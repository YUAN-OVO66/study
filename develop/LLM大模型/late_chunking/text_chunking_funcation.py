from docx.enum.text import WD_ALIGN_PARAGRAPH

def check_if_title(paragraph):
    # number_list = ["1","2","3","4","5","6","7","8","9","0", "一","二","三","四","五","六","七","八","九","十"]
    number_list = [ "一","二","三","四","五","六","七","八","九","十"]
    chars_list =[" ", "章", "节", "项", "段", "、", "。",".","：",":", ";","；"]
    for i in range(len(number_list)):
        # 如果段落中包含数字列表中的数字
        if paragraph.rfind(number_list[i]) != -1:
            # 如果数字后面还有字符
            if len(paragraph) > paragraph.rfind(number_list[i]) +1 :
                # 如果数字后面的字符在字符列表中
                if paragraph[paragraph.rfind(number_list[i]) +1]  in  chars_list:
                    return True
            else:
                return True
    return  False
 