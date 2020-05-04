from tkinter import *
import requests

class Dict:

    def __init__(self):

        self.root = Tk()

        self.root.title("Dictionary")
        self.root.minsize(400,600)
        #self.root.maxsize(400,600)
        self.root.configure(background = "#a55eea")

        label = Label(self.root,text = "Dictionary",bg = "#a55eea", fg = "#ffffff")
        label.configure(font = ("Constantia" , 22, "bold"))
        label.pack(pady = (30,10))

        self.word = Entry(self.root)
        self.word.pack(ipadx=40,ipady=10)

        click = Button(self.root,text="Search",bg = "#ffffff",width = 28, height = 2,command = lambda:self.fetch())
        click.pack(pady = (10,20))

        self.result = Label(self.root,bg = "#a55eea", fg = "#ffffff")
        self.result.configure(font = ("Constantia" , 14))
        self.result.pack(pady = (5,10))

        self.root.mainloop()

    def fetch(self):
        word = self.word.get()
        url="https://dictionaryapi.com/api/v3/references/collegiate/json/{}?key=77dab78d-7869-45a1-ac66-4adfa34893d0".format(word)
        response = requests.get(url)
        data = response.json()
        meaning = data[0]['shortdef'][0]
        
        self.result.configure(text = meaning)

        

ob = Dict()
