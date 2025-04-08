from pymilvus import MilvusClient

# 1. 连接
client=MilvusClient(
            uri="http://127.0.0.1:19530",
        )

def insert_data(data,text):
        results = client.query(
            collection_name="test",
            filter="",  # 无过滤条件
            output_fields=["id"],  # 只返回id字段
            limit=10000  # 根据实际数据量调整
        )
        # 2. 提取所有ID并找到最大值
        if results:  # 如果集合非空
            existing_ids = [item["id"] for item in results]  # 正确获取id字段的方式
            max_id = max(existing_ids)
        else:  # 如果集合为空
            max_id = 0
        next_id = max_id + 1
        data1 = {"id": next_id,"vector": data,"text": text}
        client.insert(
        collection_name="test",
        data=data1
        )
        print("插入成功")

# res = client.describe_collection(
#     collection_name="test"
# )
#
# print(res)

# 3. 查询
# print(client.query(
#     collection_name="test",
#     filter="text == '测试文本'"
# ))

# # 4. 搜索
# print(client.search(
#     collection_name="test",
#     data=[0.15, 0.25, 0.35],  # 搜索相似向量
#     limit=2
# ))
#
#5. 清理
client.delete(collection_name="test", ids=[456413634609964996])