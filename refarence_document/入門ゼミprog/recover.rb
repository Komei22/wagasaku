require "csv"

CSV.open("result.csv", "w") {|csv|
  csv_elm = Array.new
  open("nyumon.sol") {|sol|
    id = ""
    while line = sol.gets
      word = line.split
      next if word[0] == "#" 
      if id != word[0][0..11] then
        id = word[0][0..11]
        csv << csv_elm
        csv_elm.clear
      end
      csv_elm << word[1]
    end
  }
  csv << csv_elm
}
