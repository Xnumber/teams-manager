#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要插入資料的資料庫名稱作為參數。"
	exit 1
fi
echo -e "\033[1;36m插入租戶 demo 資料...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF

INSERT INTO "tenants" (name, description)
VALUES
	('研發室', '第一個租戶的描述'),
	('資訊部', '第二個租戶的描述'),
	('總經理室', '第三個租戶的描述')
ON CONFLICT (name) DO NOTHING;
EOF
