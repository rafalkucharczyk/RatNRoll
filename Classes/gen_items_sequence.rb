#!/usr/bin/ruby

Struct.new('Sequence', :speedup, :slowdown, :hover, :halve, :break, :repeat)

config = {
    sequences: [
        Struct::Sequence.new(5, 2, 2, 1, 0, 5),
        Struct::Sequence.new(4, 2, 2, 2, 0, 10),
        Struct::Sequence.new(3, 2, 2, 2, 1, 25),
        Struct::Sequence.new(3, 1, 1, 3, 2, 100)
    ]
}

result = config[:sequences].map do |x|
  fail if x.speedup + x.slowdown + x.hover + x.halve + x.break != 10

  l = [0] * x.speedup + [1] * x.slowdown + [2] * x.hover + [3] * x.halve + [4] * x.break

  x.repeat.times.map do
    l.shuffle
  end
end

puts "std::vector<int> itemsSequence = {#{result.flatten.join(', ')}};"
