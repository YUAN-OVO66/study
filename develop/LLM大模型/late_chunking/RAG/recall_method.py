from pymilvus import MilvusClient

# 连接到 Milvus
client = MilvusClient(
    uri="http://10.10.60.14:19530",
    db_name="uoh"
)
def vector_recall(query_embedding, top_k=5):
    """向量召回：语义相似度搜索"""
    results = client.search(
        collection_name="assessment_report_nlp_bert",
        data=[query_embedding],
        limit=top_k,
        output_fields=["id", "txt"],
        search_params={"metric_type": "L2", "params": {"nprobe": 5}}
    )
    return [(hit["entity"]["id"], hit["distance"]) for hit in results[0]]

def keyword_recall(query_text, top_k=5):
    """关键词召回：使用标量字段过滤（简化版BM25模拟）"""
    results = client.query(
        collection_name="assessment_report_nlp_bert",
        filter=f"txt like '%{query_text}%'",
        limit=top_k,
        output_fields=["id"]
    )
    return [(item["id"], 1.0) for item in results]  # 假设相关性得分为1.0


def reciprocal_rank_fusion(results_list, k=60):
    """RRF 重排序算法"""
    scores = {}
    for results in results_list:
        for rank, (doc_id, _) in enumerate(results, 1):
            scores[doc_id] = scores.get(doc_id, 0) + 1.0 / (k + rank)

    # 按分数降序排序
    return sorted(scores.items(), key=lambda x: x[1], reverse=True)


def hybrid_search_rrf(query_text, query_embedding, top_k=5):
    # 多路召回
    vector_results = vector_recall(query_embedding, top_k * 2)  # 扩大召回量
    keyword_results = keyword_recall(query_text, top_k * 2)

    # RRF 融合
    fused_results = reciprocal_rank_fusion([vector_results, keyword_results])

    # 获取最终结果详情
    final_ids = [doc_id for doc_id, _ in fused_results[:top_k]]
    final_results = client.get(
        collection_name="assessment_report_nlp_bert",
        ids=final_ids,
        output_fields=["id", "txt"]
    )
    return final_results

# # 模拟查询
# text = ("学校成立本科教育教学审核评估评建工作领导小组")
# query_text = text
# query_vector = ollamaEmbedding.ollama_get_embeddings(text)
# # 执行搜索
# results = hybrid_search_rrf(query_text, query_vector)
# for item in results:
#     print(f"ID: {item['id']}, Text: {item['txt']}")
