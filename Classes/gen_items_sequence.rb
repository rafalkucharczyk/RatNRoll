#!/usr/bin/ruby

Struct.new('Sequence', :speedup, :slowdown, :hover, :halve, :repeat)

config = {
    sequences: [
        Struct::Sequence.new(5, 2, 2, 1, 1),
        Struct::Sequence.new(4, 2, 2, 2, 5),
        Struct::Sequence.new(3, 2, 1, 4, 25),
        Struct::Sequence.new(3, 1, 1, 5, 50)
    ]
}

result = config[:sequences].map do |x|
  fail if x.speedup + x.slowdown + x.hover + x.halve != 10

  l = [0] * x.speedup + [1] * x.slowdown + [2] * x.hover + [3] * x.halve

  x.repeat.times.map do
    l.shuffle
  end
end

puts "std::vector<int> itemsSequence = {#{result.flatten.join(', ')}};"
