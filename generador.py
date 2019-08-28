import random
import sys
import pdb

def generate(cities):
	if len(cities) < 2:
		return []

	mixed_cities = cities.copy()
	random.shuffle(mixed_cities)
	mixed_cities.append(mixed_cities[0])

	city_count = len(cities)

	# initial "best" path
	paths = [(mixed_cities[0], mixed_cities[1], str(random.randint(1, 5)))]
	i = 1
	while i < city_count:
		paths.append((
			mixed_cities[i],
			mixed_cities[(i+1)%city_count],
			str(random.randint(1, 5))
			))

		i += 1

	min_weight = sum((int(c[2]) for c in paths))

	yield paths, min_weight

	# extra "filler" paths
	max_paths = (city_count * (city_count - 1)) / 2

	yield paths, min_weight


if __name__ == '__main__':
	cities = sys.argv[1:]

	gen = generate(cities)


	paths, min_weight = next(gen)

	print('Solution: ')
	for path in paths:
		print(','.join(path))
	print('Minimum weight: ' + str(min_weight) + '\n')


	print('Cities:\n' + ', '.join(cities) + '\nPaths:')

	paths, min_weight = next(gen)
	for path in paths:
		print(','.join(path))

