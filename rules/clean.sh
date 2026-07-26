#!/bin/sh

for option in $*; do
  case $option in
    --system=*) SYSTEM=`echo $option | sed "s@[^=]*=@@"`;;
    --mode=*) MODE=`echo $option | sed "s@[^=]*=@@"`;;
    --home=*) HOME=`echo $option | sed "s@[^=]*=@@"`;;
  esac
done

if test x = x$MODE -o x = x$SYSTEM; then
  echo "missig options, need --system=<ident> and --mode=<ident>"
  exit
fi

DIRS="."
if test x$HOME = x"."; then
    case "$MODE" in
    mostlyclean)
        DIRS=`find . -type d -maxdepth 1 -print | grep -v "^\.\(/lib\|\)$"` ;;

    realclean)
	rm -fr config-$SYSTEM ;;

    distclean)
	rm -fr config-* ;;
    esac
fi

# remove all files that can be remade
if test $MODE = distclean; then
    for AUTO in `find $DIRS -name .auto -print`; do
        DIR=`dirname $AUTO`
        for PATTERN in `(cd $DIR && cat .auto)`; do
            rm -f $PATTERN
        done
    done
    # remvoe the dependency-files
    rm -f `find $DIRS -name .depend`
fi


if test $MODE = distclean; then
    # remove all directories that were created automatically
    rm -fr `find $DIRS -name .system -type f -exec dirname {} \;`
    # remove all directories that were created automatically
    rm -f `find $DIRS -name .depend -type f -print`
else
    # remove the binary-directories of the current SYSTEM
    rm -fr `find $DIRS -name $SYSTEM -type d -print`
fi
