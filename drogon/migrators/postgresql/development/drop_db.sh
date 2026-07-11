DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo -e "\033[1;31m請提供要刪除的資料庫名稱作為參數。\033[0m"
	exit 1
fi
echo -e "\033[1;33mDB: ${DB_NAME}已存在，正在刪除資料庫 $DB_NAME...\033[0m"
sudo -u postgres dropdb "$DB_NAME"