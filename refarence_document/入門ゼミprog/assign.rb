require "csv"

first_num = 9
second_num = 8
constraints = 5

data = CSV.read("nyumon2.csv")

name = data.shift
name.shift

weight = {}
data.each{ |line|
  id = line.shift
  weight[id] = line
}

lp = open("nyumon.lp", "w")

# objective
lp.puts("minimize")
weight.each{ |id, line|
  t_idx = 0
  line.each{ |coef|
    lp.printf "+ %s x_%s_%s ", coef, id, t_idx
    t_idx += 1
  }
  lp.puts
}

# subject to
lp.puts
lp.puts("subject to")
weight.each_key{ |id|
  first_num.times{ |t_idx|
    lp.printf " + x_%s_%s", id, t_idx
  }
  lp.puts " = 1"
  second_num.times{ |t_idx|
    lp.printf " + x_%s_%s", id, first_num + t_idx
  }
  lp.puts " = 1"
}

weight.each{ |id, line|
  t_idx = 0
  line.each{ |coef|
    lp.printf "+ %s x_%s_%s ", coef, id, t_idx
    t_idx += 1
  }
  lp.printf "<= %s\n", constraints  
}

t_num = first_num + second_num
t_num.times{ |t_idx|
  weight.each_key{ |id|
    lp.printf " + x_%s_%s", id, t_idx
  }
  if t_idx < first_num then
    lp.puts(" >= 8")
  else
    lp.puts(" >= 9")
  end
  weight.each_key{ |id|
    lp.printf " + x_%s_%s", id, t_idx
  }
  if t_idx < first_num then
    lp.puts(" <= 9")
  else
    lp.puts(" <= 10")
  end
}


# binary
lp.puts
lp.puts("binary")
weight.each_key{ |id|
  t_num.times{ |t_idx|
    lp.printf " x_%s_%s", id, t_idx
  }
  lp.puts
}

# end
lp.puts("end")
lp.close
