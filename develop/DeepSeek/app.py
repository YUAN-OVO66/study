import streamlit as st
import DeepSeek

def main():

    st.set_page_config(
        page_title="小爨智能体",
        page_icon="🌟",
        layout="wide"
    )

    st.title("小爨智能体")
    st.image("logo.png", width=100)
    prompt = st.chat_input("请输入你的问题：")


    if prompt:
        # 记录用户消息
        user_message = st.chat_message("human")
        user_message.write(prompt)
        try:
            # 调用 AI 回复
            ai_reply = DeepSeek.chat_LLM(prompt)
            ai_message = st.chat_message("ai")
            ai_message.write(ai_reply)
        except Exception as e:
            st.error("AI 回复时出错，请稍后再试！")

if __name__ == "__main__":
    main()