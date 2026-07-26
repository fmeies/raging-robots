#!/bin/sh

for option in $*; do
  case $option in
    --home=*) RAGINGROBOTS_HOME=`echo $option | sed "s@[^=]*=@@"`;;
    --target-dir=*) TARGET_DIR=`echo $option | sed "s@[^=]*=@@"`;;
    --version=*) VERSION=`echo $option | sed "s@[^=]*=@@"`;;
  esac
done

if test x = x$RAGINGROBOTS_HOME -o x = x$VERSION; then
  echo "missig options, need --home=<ident> and --mode=<version>"
  exit
fi

if test x = x$TARGET_DIR; then
    TARGET_DIR=.
fi
TARGET=$TARGET_DIR/ragingrobots-$VERSION.tar.gz

cd $RAGINGROBOTS_HOME/..

for X in `find ragingrobots -name .auto -print`; do
    TMP_DIR=`dirname $X`
    Z=""
    for Y in `(cd $TMP_DIR && cat .auto)`; do
        Z="$Z `echo $TMP_DIR/$Y`"
    done
    AUTO_FILES="$AUTO_FILES $Z"
done
 
# find the files that shall not be stored
SYSTEM_DIRS=`find ragingrobots -name .system -exec dirname {} \;`
SYSTEM_FILES=`for X in $SYSTEM_DIRS; do find $X -type f -print; done`
REVISION_FILES=`find ragingrobots -name "*,v" -print`
DEPEND_FILES=`find ragingrobots -name .depend -print`
EXCLUDE_FILES=$TARGET_DIR/ragingrobots_exclude
echo $SYSTEM_FILES $DEPEND_FILES $REVISION_FILES $AUTO_FILES \
  | tr " " "\012" | sort >$EXCLUDE_FILES

# find all files
ALL_FILES=$TARGET_DIR/ragingrobots_all
find ragingrobots -type f -print | sort > $ALL_FILES

# compute the files that shall be stored
TARGET_FILES=`comm -3 $ALL_FILES $EXCLUDE_FILES`

# create the archive
echo "creating archive"
tar cf - $TARGET_FILES 2>/dev/null | gzip -9 > $TARGET

rm -f $ALL_FILES $EXCLUDE_FILES
