#! /usr/bin/python3
import cgi, cgitb

form = cgi.FieldStorage()

blue = form.getvalue('blue')
red = form.getvalue('red')

# print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>Hello - CGI Program</title>")
print ('</head>')
print ('<body>')
print ("<h2>Blue is  %s</h2>" % (blue))
print ("<h2>Red is  %s</h2>" % (red))
print ('</body>')
print ('</html>')