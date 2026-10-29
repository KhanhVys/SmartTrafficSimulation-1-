#pragma once

#include "core/Path.h"

// Lộ trình của một xe: đường đi + các mốc quan trọng (đo theo quãng đường s).
//   s = 0                    : điểm sinh xe (rìa cửa sổ)
//   s = stopLineS            : vạch dừng
//   s = intersectionStartS   : mép vào ngã tư
//   s = intersectionEndS     : mép ra ngã tư (sau khi hết đoạn rẽ)
struct Route {
    Path path;
    double stopLineS = 0.0;
    double intersectionStartS = 0.0;
    double intersectionEndS = 0.0;
};
