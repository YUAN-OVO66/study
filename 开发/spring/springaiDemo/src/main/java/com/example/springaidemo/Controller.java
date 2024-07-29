package com.example.springaidemo;


import jakarta.annotation.Resource;
import org.springframework.ai.chat.model.ChatResponse;
import org.springframework.ai.chat.prompt.Prompt;
import org.springframework.ai.ollama.OllamaChatModel;
import org.springframework.ai.ollama.api.OllamaOptions;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class Controller {
    @Resource
    private OllamaChatModel ollamaChatModel;
    @RequestMapping(value = "/ai/ollama")
    public Object ollama(@RequestParam(value = "msg")String msg){
        String called=ollamaChatModel.call(msg);
        System.out.println(msg);
        System.out.println(called);
        return called;
    }
    @RequestMapping(value = "/ai/ollama3.1")
    public Object ollama2(@RequestParam(value = "msg")String msg){
        ChatResponse chatResponse=ollamaChatModel.call(new Prompt(msg, OllamaOptions.create()
                .withModel("llama3.1:latest")
                .withTemperature(0.4F)));
        System.out.println(chatResponse.getResult().getOutput().getContent());
        return chatResponse.getResult().getOutput().getContent();
    }
}
