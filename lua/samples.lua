-- Consts and prep
PI = 3.14159265358979323846;
NORMAL95CONFIDENCE = 1.6448536269514722;
math.randomseed(1234)

-- Random distribution functions
function sample_normal_0_1()
  local u1 = math.random()
	local u2 = math.random()
	local result = math.sqrt(-2 * math.log(u1)) * math.sin(2 * PI * u2)
	return result
end

function sample_normal(mean, sigma)
	return mean + (sigma * sample_normal_0_1())
end

function sample_uniform(min, max)
  return math.random() * (max - min) + min
end

function sample_lognormal(logmean, logsigma)
	return math.exp(sample_normal(logmean, logsigma))
end

function sample_to(low, high)
	local loglow = math.log(low);
	local loghigh = math.log(high);
	local logmean = (loglow + loghigh) / 2;
	local logsigma = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE);
	return sample_lognormal(logmean, logsigma, seed);
end

-- Mixture
function mixture(samplers, weights, n)
	assert(#samplers == #weights)
  local l = #weights
	local sum_weights = 0
	for i = 1, l, 1 do
		-- ^ arrays start at 1
		sum_weights = sum_weights + weights[i]
	end
  local cumsummed_normalized_weights = {}
	table.insert(cumsummed_normalized_weights, weights[1]/sum_weights)
	for i = 2, l, 1 do
		table.insert(cumsummed_normalized_weights, cumsummed_normalized_weights[i-1] + weights[i]/sum_weights)
	end
	
	local result = {}
	for i = 1, n, 1 do
		r = math.random()
		local i = 1
		while r > cumsummed_normalized_weights[i] do
			i = i+1
		end
		table.insert(result, samplers[i]())
	end
	return result
end


-- Main 
p_a = 0.8
p_b = 0.5
p_c = p_a * p_b

function sample_0() return 0 end
function sample_1() return 1 end
function sample_few() return sample_to(1, 3) end
function sample_many() return sample_to(2, 10) end

samplers = {sample_0, sample_1, sample_few, sample_many}
weights = { (1 - p_c), p_c/2, p_c/4, p_c/4 }

n = 1000000
result = mixture(samplers, weights, n)
sum = 0
for i = 1, n, 1 do
	sum = sum + result[i]
	-- print(result[i])
end
print(sum/n)
