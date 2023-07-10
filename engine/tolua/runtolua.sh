BIN=$(command -v tolua++)

if [ $? != 0 ]; then
  BIN=$(command -v toluapp)
  if [ $? != 0 ]; then
    echo "tolua++ is not installed"
    exit 1
  fi
fi

$BIN -L stratagus.lua -o tolua.cpp stratagus.pkg
