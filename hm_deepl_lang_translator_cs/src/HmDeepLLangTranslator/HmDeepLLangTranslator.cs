using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;

using Hidemaru;

namespace HmDeepLLangTranslator
{
    internal static class JsonUtilityExtension
    {
        public static string ToJson<T>(this T obj) where T : class
        {
            var serializer = new DataContractJsonSerializer(typeof(T));

            using (var stream = new MemoryStream())
            {
                serializer.WriteObject(stream, obj);
                return Encoding.UTF8.GetString(stream.ToArray());
            }

        }

        public static T FromJson<T>(this T obj, string json) where T : class
        {
            using (var stream = new MemoryStream(Encoding.UTF8.GetBytes(json)))
            {
                var serializer = new DataContractJsonSerializer(typeof(T));
                return serializer.ReadObject(stream) as T;
            }
        }
    }


    [DataContract]
    internal partial class PostDownloadData
    {

        [DataMember(Name = "translations")]
        public Translations[] translations;
    }

    [DataContract(Name = "translations")]
    internal partial class Translations
    {

        [DataMember(Name = "detected_source_language")]
        public string detected_source_language;

        [DataMember(Name = "text")]
        public string text;
    }



    public class HmDeepLLangTranslator
    {

        public static string TryTranslate(string auth_key, string source_lang, string target_lang)
        {
            try
            {
                string text = Hm.Edit.SelectedText;
                if (String.IsNullOrWhiteSpace(text))
                {
                    text = Hm.Edit.TotalText;
                }
                return _TryTranslate(auth_key, text, source_lang, target_lang);
            }
            catch (Exception ex)
            {
                return "翻訳エラー:" + ex.Message;
            }
        }

        private static string _TryTranslate(string auth_key, string text, string source_lang, string target_lang)
        {
            using (var httpClient = new HttpClient())
            {
                using (var request = new HttpRequestMessage(new HttpMethod("POST"), "https://api-free.deepl.com/v2/translate"))
                {
                    /*
                     * こちらは%や\が組み合わさると逆に上手く動作しなくなる。
                    List<KeyValuePair<string, string>> parameters = new List<KeyValuePair<string, string>>();
                    parameters.Add(new KeyValuePair<string, string>("auth_key", auth_key));
                    parameters.Add(new KeyValuePair<string, string>("text", text));
                    parameters.Add(new KeyValuePair<string, string>("source_lang", source_lang));
                    parameters.Add(new KeyValuePair<string, string>("target_lang", target_lang));
                    HttpContent httpContent = new FormUrlEncodedContent(parameters);
                    request.Content = httpContent;
                    */

                    // 原始的だがこちらの方がまともに動作する
                    var contentList = new List<string>();
                    contentList.Add($"auth_key={auth_key}");
                    contentList.Add($"text={text}");
                    contentList.Add($"source_lang={source_lang}");
                    contentList.Add($"target_lang={target_lang}");

                    request.Content = new StringContent(string.Join("&", contentList));
                    request.Content.Headers.ContentType = MediaTypeHeaderValue.Parse("application/x-www-form-urlencoded");

                    ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls11 | SecurityProtocolType.Tls12 | SecurityProtocolType.Tls13;
                    var response = httpClient.SendAsync(request);
                    var result_json = response.Result.Content.ReadAsStringAsync().Result;
                    response.Wait();
                    PostDownloadData result_data_obj = JsonUtilityExtension.FromJson(new PostDownloadData(), result_json);
                    var answer_text = result_data_obj.translations[0].text;
                    return answer_text;
                }
            }
        }

    }
}
