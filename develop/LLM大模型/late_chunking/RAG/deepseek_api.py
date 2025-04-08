from openai import OpenAI
from dotenv import load_dotenv
import os

load_dotenv()
deepseek_api_key = os.getenv("DEEPSEEK_API_KEY","YUAN")

def deepseek_chat(message):
    try:
        client = OpenAI(
            base_url="https://api.deepseek.com",
            api_key=deepseek_api_key
        )
        response = client.chat.completions.create(
            model="deepseek-chat",
            messages=[
                {"role": "user","content": message}
            ],
            stream=False
        )
        return response.choices[0].message.content
    except Exception as e:
        print(f"出错了：{e}")
        return None

def deepseek_chat_RAG(message,knowledge_text):
    try:
        client = OpenAI(
            base_url="https://api.deepseek.com",
            api_key=deepseek_api_key
        )

        response = client.chat.completions.create(
            model="deepseek-chat",
            messages=[
                {"role": "system","content": "你是一位知识渊博的AI助手，以下是从知识库中检索到的相关内容"+knowledge_text+"请根据这些信息，准确、简洁地回答用户的问题。如果检索内容中没有明确的信息，请说明“根据当前信息无法确定”。"},
                {"role": "user","content": message}
            ],
            stream=False
        )
        return response.choices[0].message.content
    except Exception as e:
        print(f"出错了：{e}")
        return None

