echo "-----------------------------------------------------------"

execDir=`pwd`

if [ "$1" = "" ]
then
  echo "No directory given, default to current"
  Directory="."
else
  echo "| Test id : $1"
  if [ -d "$1" ]
  then
     Directory="$1"
  else
     echo "$1 is not a directory. Exiting."
     exit 2
  fi
fi

cd $Directory

# Removing old values
rm -f *.REAL
rm -f *.DIFF

if [ -r "description" ]
then
  echo "-----------------------------------------------------------"
  echo "Description :"
  fold -w 60 -s description
  echo "-----------------------------------------------------------"
fi

nTestCount=0
nSuccesfulTests=0
nStrResult="$1 "

if [ -r "run" ]
then
  sRun=`cat run`
else
  echo "No run file found. Exiting."
  exit 2
fi

echo $sRun

# stdin has been specified
if [ -r "std.in" ]
then
  sRun="$sRun <std.in"
fi

# stdout has been specified
if [ -r "std.out" ]
then
  sRun="$sRun >temp.txt"
fi

# stderr has been specified
if [ -r "stderr.out" ]
then
  sRun="$sRun 2>temperr.txt"
fi

# execute the command line
eval $sRun
returnCode=$?

resultGlobal="Ok"

# compare return code if concerned
resultRC="Not tested"
if [ -r "returncode" ]
then
  if [ "$returnCode" = `cat returncode` ]
  then
    echo "                                       Return Code : PASSED"
    resultRC="Ok"
  else
    echo "$returnCode" > returnCode.REAL
    echo "                                       Return Code : FAILED"
    resultRC="Failure"
    resultGlobal="Failure"
  fi
fi

# compare stdout if concerned
resultOut="Not tested"
if [ -r "std.out" ]
then
  diff -wB temp.txt std.out > stdout.DIFF
  if [ $? -eq 0 ]
  then
    rm stdout.DIFF
    echo "                                       Stdout      : PASSED"
    resultOut="Ok"
  else
    cp temp.txt stdout.REAL
    echo "                                       Stdout      : FAILED"
    resultOut="Failure"
    resultGlobal="Failure"
  fi
  # clean temporary out file
  rm temp.txt
fi

# compare stderr if concerned
resultErr="Not tested"
if [ -r "stderr.out" ]
then
  diff -wB temperr.txt stderr.out > stderr.DIFF
  if [ $? -eq 0 ]
  then
    rm stderr.DIFF
    echo "                                       Stderr      : PASSED"
    resultErr="Ok"
  else
    cp temperr.txt stderr.REAL
    echo "                                       Stderr      : FAILED"
    resultErr="Failure"
    resultGlobal="Failure"
  fi
  # clean temporary out file
  rm temperr.txt
fi

# compare files created if concerned
resultFiles="Not tested"
if ls *.outfile &> /dev/null
then
  number=1
  for i in *.outfile
  do
    fileName=`basename $i .outfile`
    if [ -r $fileName ]
    then
      diff -wB $i $fileName
      if [ $? -eq 0 ]
      then
        echo "                                       File #$number     : PASSED"
      else
        echo "                                       File #$number     : FAILED"
        resultFiles="Failure"
        resultGlobal="Failure"
      fi
      rm $fileName
    else
      echo "                                       File #$number     : FAILED"
      resultFiles="Failure"
      resultGlobal="Failure"
    fi
    let "number=$number+1"
  done
  if [ $resultFiles = "Not tested" ]
  then
    resultFiles="Ok"
  fi
fi

echo "                                       --------------------"
if [ $resultGlobal = "Failure" ]
then
  echo "                                       Global      : FAILED"
else
  echo "                                       Global      : PASSED"
fi
echo "-----------------------------------------------------------"
echo

cd $execDir

# log result in $2 if filename provided
if [ "$2" != "" ]
then
  if [ ! -w "$2" ]
  then
    touch $2
  fi
  if [ -w "$2" ]
  then
    echo "$Directory;$resultRC;$resultOut;$resultErr;$resultFiles;$resultGlobal" >>$2
  fi
fi

if [ $resultGlobal = "Failure" ]
then
   exit 0
fi

if [ $resultGlobal = "Ok" ]
then
   exit 1
fi
