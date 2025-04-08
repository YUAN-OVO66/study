from RAG import  deepseek_api
from RAG import recall_method
from milvus import ollama_embedding




# 模拟查询
text = ("四个回归")
query_text = text
query_vector = ollama_embedding.ollama_get_embeddings(text)
# 执行搜索
results = recall_method.hybrid_search_rrf(query_text, query_vector)
for item in results:
    #print(f"ID: {item['id']}, Text: {item['txt']}")
    #转换为字符串
    item['txt'] = str(item['txt'])
print(deepseek_api.deepseek_chat_RAG(text,item['txt']))