# -*- coding: utf-8 -*-

#
# HmDeepLLangTranslator.mac v1.1.0.1
# Copyright (c) 2022 Akitsugu Komiyama
# under the MIT License
#
# 秀丸 v8.73以上
# 要 hmPython3 1.8.1.1 以上
#

from hmPython import hm
import json
import urllib.request


#-------------------------------------------------------------

def TranslateByPost(auth_key, text, s_lang, t_lang):
    headers = {'Content-Type': 'application/x-www-form-urlencoded; utf-8'}

    script_url = "https://api-free.deepl.com/v2/translate";

    params = {
        "auth_key": auth_key,
        "text": text,
        "source_lang": s_lang,
        "target_lang": t_lang
    }
    encode_data = urllib.parse.urlencode(params).encode('utf-8')
    req = urllib.request.Request(
        script_url,
        method="POST",
        data=encode_data,
        headers=headers
    )

    try:
        with urllib.request.urlopen(req) as res:
            res_json = json.loads(res.read().decode('utf-8'))
            return res_json["translations"][0]["text"]
    except urllib.error.HTTPError as e:
        return "翻訳エラー:" + str(e)

    return "翻訳エラー";
