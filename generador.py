import random
import sys
import pdb

def generate(cities):
  if len(cities) < 2:
    raise Exception

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


def create_full(city_count):
  matrix = []
  for i in range(city_count):
    matrix.append([[]] * (i + 1))

    for j in range(i + 1):
      matrix[i][j] = random.randint(1, 20)

  return matrix


def write_file(matrix, file):
  with open(file, 'w') as f:
    f.write('Ciudades\n')

    for i in range(len(matrix) - 1):
      f.write(str(i+1) + ', ')
    f.write(str(len(matrix)))

    f.write('\nCostos')

    for i,column in enumerate(matrix):
      for j in range(len(column)):
        if i != j:
          f.write('\n' + str(i+1) + ',' + str(j+1) + ',' + str(matrix[i][j]))

if __name__ == '__main__':
  mat = create_full(int(sys.argv[1]))

  for column in mat:
    print(column)


  write_file(mat, 'entrada3.txt')
#   cities = sys.argv[1:]

#   gen = generate(cities)


#   paths, min_weight = next(gen)

#   print('Solution: ')
#   for path in paths:
#     print(','.join(path))
#   print('Minimum weight: ' + str(min_weight) + '\n')


#   print('Cities:\n' + ', '.join(cities) + '\nPaths:')

#   paths, min_weight = next(gen)
#   for path in paths:
#     print(','.join(path))

