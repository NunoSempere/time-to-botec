Instead of

```C
#pragma omp parallel private(i, sample_index, split_array_length)
{
    #pragma omp for
    for (i = 0; i < n_threads; i++) {
        split_array_length = split_array_get_length(i, N, n_threads);
        for (int j = 0; j < split_array_length; j++) {
            results[i][j] = sampler(seeds[i]);
        }
    }
}

```

Algo como

```C
#pragma omp parallel private(i, sample_index, bounds)
{
    #pragma omp for
    for (i = 0; i < n_threads; i++) {
        int bounds[2] = split_array_get_bounds(i, N, n_threads);
        for (int j = bound[0]; j < bounds[1] + 1; j++) {
            // o j < bounds[1], no se si el +1 va a ser más elegante
            // dentro o fuera, aunque algo me dice que dentro
            results[j] = sampler(seeds[i]);
        }
    }
}

```

Por qué? Porque esto hace que la estructura subyacente sea un solo array, 
lo cual implica que no *tengo* que utilizar split_array_functions especializadas 
si no quiero
