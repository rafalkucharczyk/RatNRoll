#!/usr/bin/ruby

Struct.new('Sequence', :speedup, :slowdown, :hover, :halve, :break, :repeat)

config = {
    sequences: [
        Struct::Sequence.new(5, 2, 2, 0, 1, 1),
        Struct::Sequence.new(4, 2, 2, 1, 1, 5),
        Struct::Sequence.new(2, 2, 1, 4, 1, 25),
        Struct::Sequence.new(3, 1, 1, 4, 1, 50)
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
