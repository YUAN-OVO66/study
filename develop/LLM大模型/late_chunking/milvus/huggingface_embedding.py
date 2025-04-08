from pymilvus.model.hybrid import BGEM3EmbeddingFunction

bge_m3_ef = BGEM3EmbeddingFunction(
    model_name='BAAI/bge-m3', # Specify the model name
    device='cpu', # Specify the device to use, e.g., 'cpu' or 'cuda:0'
    use_fp16=False # Specify whether to use fp16. Set to `False` if `device` is `cpu`.
)

def huggingface_embedding(text):
    docs_embeddings = bge_m3_ef.encode_documents(text)
    return docs_embeddings["dense"]  # 返回整个 dense 向量列表
