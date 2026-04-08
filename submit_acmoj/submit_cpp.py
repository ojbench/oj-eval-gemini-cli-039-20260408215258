import os
import requests

token = os.environ.get("ACMOJ_TOKEN")
headers = {
    "Authorization": f"Bearer {token}",
    "Content-Type": "application/x-www-form-urlencoded",
    "User-Agent": "ACMOJ-Python-Client/2.2"
}
data = {"language": "cpp", "code": "int main() { return 0; }"}
response = requests.post("https://acm.sjtu.edu.cn/OnlineJudge/api/v1/problem/2621/submit", headers=headers, data=data)
print(response.status_code)
print(response.text)
