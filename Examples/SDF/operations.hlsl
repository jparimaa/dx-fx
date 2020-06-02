float intersect(in float a, in float b)
{
    return max(a, b);
}

float conjunct(in float a, in float b)
{
    return min(a, b);
}

float smoothConjunct(in float a, in float b, float k)
{
    float res = exp(-k * a) + exp(-k * b);
    return -log(max(0.0001, res)) / k;
}

float subtract(in float a, in float b)
{
    return max(a, -b);
}
