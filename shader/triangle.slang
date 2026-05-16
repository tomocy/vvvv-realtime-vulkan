[[shader("vertex")]]
float32_t4 vertexMain(uint32_t vertexId : SV_VertexID) : SV_Position {
    constexpr float32_t3 positions[3] = {
        float32_t3(-0.5, -0.5, 0.0),
        float32_t3(0.5, -0.5, 0.0),
        float32_t3(0.0, 0.5, 0.0)
    };

    return float32_t4(positions[vertexId], 1.0);
}

[[shader("fragment")]]
float32_t4 fragmentMain() : SV_Target {
    return float32_t4(1.0, 0.0, 0.0, 1.0);
}
