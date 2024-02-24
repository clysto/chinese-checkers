import random

# 棋盘设置
BOARD_SLOTS = 81  # 棋盘位置总数
PIECE_TYPES = 3  # 棋子类型：红、绿、空

# 生成Zobrist表
zobrist_table = [
    [random.getrandbits(64) for _ in range(PIECE_TYPES)] for _ in range(BOARD_SLOTS)
]

# 转换为C++代码字符串
zobrist_table_cpp = "uint64_t ZobristTable[{}][{}] = {{\n".format(
    BOARD_SLOTS, PIECE_TYPES
)
for row in zobrist_table:
    zobrist_table_cpp += (
        "    { " + ", ".join("0x{:016x}ULL".format(value) for value in row) + " },\n"
    )
zobrist_table_cpp += "};\n"

print(zobrist_table_cpp)
