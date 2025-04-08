from pymilvus import MilvusClient, DataType

client = MilvusClient(
    uri="http://localhost:19530",
)

schema = MilvusClient.create_schema(
    auto_id=False,
    enable_dynamic_field=True,
)

schema.add_field(field_name="id", datatype=DataType.INT64, is_primary=True)
schema.add_field(field_name="vector", datatype=DataType.FLOAT_VECTOR, dim=1024)
schema.add_field(field_name="varchar", datatype=DataType.VARCHAR, max_length=1024)
