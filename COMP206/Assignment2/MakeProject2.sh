#!/bin/sh
#Change to home directory
cd
if [[ ! -d "project" ]]; then
	mkdir project
fi
cd project
if [[ ! -d "cs206" ]]; then
	mkdir cs206
fi
cd cs206
if [[ -d $1 ]]; then
	echo "This project name has already been used"
else
	mkdir -p $1/{archive,backup,docs,assets,database,source}
	cd $1/source
	if [ ! -e "backup.sh" ]; then
  		echo "#!/bin/sh" >> backup.sh
  		echo "#This file was made by MakeProject.sh to copy things to backup" >> backup.sh
  		echo "find . -type f \( -name "*.h" -or -name "*.c" \) -exec cp {} ../backup" >>backup.sh
  		chmod +755 backup.sh
	fi
fi
echo "Your project directories have been created"

