def check(input_1, input_2, input_3, input_4):
    if len(input_1.replace("/ /g", '')) == 0 or len(input_2.replace("/ /g", '')) == 0 or len(input_3.replace("/ /g", '')) == 0 or len(input_4.replace("/ /g", '')) == 0:
        print("cane")
    else:
        print("ass")

if __name__ == '__main__':
    check("a", " ", "b", "ff")