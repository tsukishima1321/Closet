print("进程启动")
import sys
sys.stdout.flush()

import easyocr
import cv2
import pymysql
import numpy as np
import os

print("环境导入完成")
sys.stdout.flush()

def readImg(img, reader) -> str:
    h,w,_ = img.shape
    croppeds = []
    max_h = 2000
    if w > 1200:
        max_h = 1000
    else:
        max_h = 2000
    for i in range(0,h,max_h):
        croppeds.append(img[i:i+max_h,:])
    res = ""
    for cropped in croppeds:
        result = reader.readtext(cropped,slope_ths=0.5,width_ths=0.7)
        for detection in result:
            res = res + detection[1].strip()
    return res

files = []
try:
    if(len(sys.argv)>3):
        files = sys.argv[2:]
    reader = easyocr.Reader(['ch_sim','en'])
    conn = pymysql.connect(host="localhost",
                           port=3306,
                           user=sys.argv[1],
                           password=sys.argv[2],
)
    cursor = conn.cursor()
    cursor.execute("use diary")
    print("数据库登录成功")
    sys.stdout.flush()
except:
    print("数据库登录失败，请检查命令")
    sys.stdout.flush()
    exit()
try:
    for file in files:
        if file.endswith("jpg") or file.endswith("png") or file.endswith("jpeg") or file.endswith("bmp"):
            cursor.execute("select href from pictures_ocr where href='"+os.path.basename(file)+"'")
            if cursor.fetchone():
                print(os.path.basename(file)+" 已存在记录")
                sys.stdout.flush()
                continue
            img = cv2.imdecode(np.fromfile(file,dtype=np.uint8),-1)
            res = readImg(img,reader)
            res = res.replace("'","")
            #print(file,":\n",res)
            sql = "insert into pictures_ocr (href,ocr_result) values ('"+os.path.basename(file)+"','"+res+"')"
            print(os.path.basename(file),":\n",res)
            sys.stdout.flush()
            cursor.execute(sql)
            conn.commit()
except Exception as e:
    print(e)
    sys.stdout.flush()
print("结束")
sys.stdout.flush()
