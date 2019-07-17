
def check(input_1, input_2, input_3, input_4):
    if len(input_1.replace("/\s/g", ''))==0 or len(input_2.replace("/\s/g", ''))==0 or len(input_3.replace("/\s/g", ''))==0 or len(input_4.replace("/\s/g", ''))==0:
        return False
    else:
        return True
