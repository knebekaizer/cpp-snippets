#!/bin/sh

cwd=`pwd`
archDir=$cwd/arch


RotateLogs() 
{
	local logs="$@"
    ${logs:="`find $cwd -type f -name *.log -maxdepth 1`"}
    [ -d $archDir ] || mkdir $archDir
    for f in $logs; do 
#      [ -s $f ] && cat $f >> $archDir/`basename $f` && rm -f $f >& /dev/null
       cat $f >> $archDir/`basename $f` && echo -n > $f
    done
}

# sh rotate.sh > rotate.log
[ -d arch ] || mkdir arch
#[ -s rotate.log ] && cat rotate.log >> arch/rotate.log && echo -n > rotate.log
for f in `find $cwd -maxdepth 1 -type f -name "*.log"`; do
#       [ -s $f ] && cat $f >> $archDir/`basename $f` && echo -n
   cat $f >> $archDir/`basename $f` && echo -n > $f
#   cat $f >> $archDir/`basename $f` && rm -f $f >& /dev/null
done

#[ -s b.log ] && cat b.log >> arch/b.log && rm b.log
 
echo `date +'%Y-%m-%d %H:%M:%S'` [INFO] bla-bla
echo `date +'%Y-%m-%d %H:%M:%S'` [DEBUG] bla-bla
echo `date +'%Y-%m-%d %H:%M:%S'` [TRACE] 111
echo `date +'%Y-%m-%d %H:%M:%S'` [TRACE] 222

