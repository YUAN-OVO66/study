import ollama

# 连接到本地Ollama服务
client = ollama.Client(host='http://localhost:11434')  # 默认端口

# 文本向量化
def ollama_get_embeddings(text):
    response = client.embeddings(
        model="bge-m3:latest",
        prompt=text
    )
    return response["embedding"]  # 提取实际的向量数据
