package samples

import "core:math"
import "core:fmt"
import "core:math/rand"
import "core:mem"
import "core:thread"
import "core:os"

N :: 1_000_000

random_to :: proc (low, high : f32, gen := context.random_generator) -> f32 {
    NORMAL95CONFIDENCE :: 1.6448536269514722
    loglow    : f32 = math.ln(low)
    loghigh   : f32 = math.ln(high)
    logmean   : f32 = (loglow + loghigh) / 2
    logsigma  : f32 = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE)
    return rand.float32_log_normal(logmean, logsigma, gen)
}

model :: proc(gen := context.random_generator) -> f32 {
    p_a, p_b, p_c : f32
    p_a, p_b = 0.8, 0.5
    p_c = p_a * p_b

    ws := [?]f32{1 - p_c, p_c / 2, p_c / 4, p_c / 4}
    p := rand.float32_uniform(0.0, 1.0, gen)
    switch p {
        case 0..<ws[0]                          : return 0.0
        case ws[0]..<ws[0]+ws[1]                : return 1.0
        case ws[0]+ws[1]..<ws[0]+ws[1]+ws[2]    : return random_to(1.0, 3.0)
        case                                    : return random_to(2.0, 10.0)
    }
}

model_multiple_payload :: struct #align(256){
    n_items : int,
    result: f32
}

model_multiple :: proc(task: thread.Task) {
    mean : f32
    payload := cast(^model_multiple_payload) task.data
    for _ in 0..<payload.n_items {
        mean += model()
    }
    payload.result = mean
}

main :: proc() {
    mean : f32
    cores := os.processor_core_count()
    payloads := make([]model_multiple_payload, cores)
    defer delete(payloads)

    pool_allocator : mem.Mutex_Allocator
    mem.mutex_allocator_init(&pool_allocator, context.allocator)

    pool : thread.Pool
    thread.pool_init(&pool, context.allocator, cores)
    thread.pool_start(&pool)
    defer thread.pool_destroy(&pool)

    for i in 0..<cores {
        payloads[i].n_items = N/cores

        thread.pool_add_task(&pool, mem.mutex_allocator(&pool_allocator), model_multiple, &payloads[i], i)
    }
    thread.pool_finish(&pool)

    for i in 0..<cores {
        mean += payloads[i].result
    }

    fmt.println("Sum(dist_mixture, N)/N = ", mean / N)
}