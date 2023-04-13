import json
import tkinter as tk
import requests
from PIL import ImageTk, Image

# replace with your API key
API_KEY = 'xxxxx'
# Full path of the image files
sun_path = r'C:\Users\Anan\Documents\GitHub\ananbose\S6\sun.png'
rain_path = r'C:\Users\Anan\Documents\GitHub\ananbose\S6\rain.png'

# replace with the name of the city you want to get the weather for
city_name = 'Seattle'

# make the API request
url = f'http://api.openweathermap.org/data/2.5/weather?q={city_name}&appid={API_KEY}&units=imperial'
response = requests.get(url)

# parse the JSON response
data = json.loads(response.text)

# create the GUI
root = tk.Tk()
root.title(f'Current weather in {city_name}')
#print(data)

# set the background image
frame = tk.Frame(root, bg="#cfe2f3")
frame.pack(fill="both", expand=True)


temp_label = tk.Label(frame, text=f'Temperature: {data["main"]["temp"]} F')
temp_label.pack()

humidity_label = tk.Label(frame, text=f'Humidity: {data["main"]["humidity"]}%')
humidity_label.pack()

wind_label = tk.Label(frame, text=f'Wind speed: {data["wind"]["speed"]} mph')
wind_label.pack()

cloud_label = tk.Label(frame, text=f'Clouds: {data["weather"][0]["description"]}')
cloud_label.pack()



# load the image files
sun_img = Image.open(sun_path)
rain_img = Image.open(rain_path)

# resize the images
sun_img = sun_img.resize((200, 200))
rain_img = rain_img.resize((200, 200))

# convert the images to PhotoImage objects
sun_image = ImageTk.PhotoImage(sun_img)
rain_image = ImageTk.PhotoImage(rain_img)

# create image label
image_label = tk.Label(root, image=None)
image_label.pack()

# update image based on weather condition
if 'rain' in data['weather'][0]['description'].lower()  or data['main']['humidity'] > 65:
    image_label.configure(image=rain_image)
elif 'sun' in data['weather'][0]['description'].lower():
    image_label.configure(image=sun_image)

root.mainloop()
