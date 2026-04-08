import os
import requests

token = os.environ.get("ACMOJ_TOKEN")
headers = {
    "Authorization": f"Bearer {token}",
    "User-Agent": "ACMOJ-Python-Client/2.2"
}
response = requests.get("https://acm.sjtu.edu.cn/OnlineJudge/api/v1/problem/2621", headers=headers)
print(response.status_code)
print(response.text)
