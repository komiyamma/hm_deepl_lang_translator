﻿<?php
#
# HmDeepLLangTranslator.php v1.0.1.1
# Copyright (c) 2022 Akitsugu Komiyama
# under the MIT License
#
# 秀丸 v8.73以上

$target_text = $Hm->Edit->getSelectedText() or $Hm->Edit->getTotalText();
$param = array(
'auth_key' => $Hm->Macro->getVar('$auth_key'),
'text' => $target_text,
'target_lang' => $Hm->Macro->getVar('$target_lang'),
);
$curl = curl_init("https://api-free.deepl.com/v2/translate");
curl_setopt($curl, CURLOPT_CUSTOMREQUEST, "POST");
curl_setopt($curl, CURLOPT_POSTFIELDS, http_build_query($param));
curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, FALSE);
curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, FALSE);
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);

$res = curl_exec($curl);
# $errno = curl_errno($curl);
# $error = curl_error($curl);
curl_close($curl);

$json = json_decode($res, true);
if ($json["translations"][0]["text"] == NULL) {
    $Hm->Macro->setVar( '$AnswerText', "翻訳エラーです" );
} else {
    # var_dump($json);
    $Hm->Macro->setVar( '$AnswerText', $json["translations"][0]["text"] );
}
?>


