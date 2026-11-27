#!/bin/sh
FILES=`ls ../Source/*.c`
xgettext -d zombie-attack -o zombie-attack.pot --keyword=_:1  $FILES 
msgmerge -U pl.po zombie-attack.pot
