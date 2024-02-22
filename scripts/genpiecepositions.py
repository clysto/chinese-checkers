start_x = 310
start_y = 30
piece_radius = 19
piece_gap = 45

sqrt3 = 3**0.5

delta_x_increase = [piece_gap / 2, piece_gap * sqrt3 / 2]
delta_y_increase = [-piece_gap / 2, piece_gap * sqrt3 / 2]

PIECE_POSITIONS = []

for p in range(81):
    x = p % 9
    y = p // 9
    pos_x = start_x
    pos_y = start_y
    # offset introduced by x
    pos_x += delta_x_increase[0] * x
    pos_y += delta_x_increase[1] * x
    # offset introduced by y
    pos_x += delta_y_increase[0] * y
    pos_y += delta_y_increase[1] * y
    PIECE_POSITIONS.append([pos_x, pos_y])

print(PIECE_POSITIONS)
