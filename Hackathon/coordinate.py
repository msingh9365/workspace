import math

class coordinate:

    def __init__(self,x,y):
        self.x = x
        self.y = y

    def __str__(self):
        return "({},{})".format(self.x,self.y)

    def distance(self,other):
        dist = math.sqrt((other.x-self.x)**2 + (other.y - self.y)**2)
        return dist

    def midpoint(self,other):
        x = (other.x+self.x)/2
        y = (other.y+self.y)/2
        return "{},{}".format(x,y)

    def distanceOrigin(self):
        dist = math.sqrt(self.x**2+self.y**2)
        return dist

    def lineEqn(self,other):
        m = (other.y-self.y)/(other.x-self.x)
        c = self.y - m*self.x
        print("Line Eqn: {}x - y + {}".format(m,c))
    
obj = coordinate(2,3)
