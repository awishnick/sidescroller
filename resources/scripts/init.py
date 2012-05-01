def init():
    hello = file('hello.txt', 'w')
    hello.write('Hello world!')
    hello.close()
