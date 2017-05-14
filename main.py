import kivy
kivy.require('1.9.0')
from kivy.app import App
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.gridlayout import GridLayout
from kivy.uix.button import Button
from kivy.uix.behaviors import ButtonBehavior
from kivy.uix.image import Image
from kivy.properties import StringProperty
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.clock import Clock, mainthread
from kivy.core.audio import SoundLoader
from plyer import notification
import threading
import urllib, json
import time
import datetime
maior_Temperatura = 0.0
menor_Temperatura = 100.0
maior_Umidade = 0.0
menor_Umidade = 100.0
class ImageButton(ButtonBehavior, Image):
	pass
class HomeScreen(Screen):
	pass
class MyScreenManager(ScreenManager):
	pass
class ReportScreen(Screen):
	maiorTemp = StringProperty()
	menorTemp = StringProperty()
	maiorUmid = StringProperty()
	menorUmid = StringProperty()
	def settings(self,i):
		global maior_Temperatura
		global menor_Temperatura
		global maior_Umidade
		global menor_Umidade
		self.maiorTemp = str(maior_Temperatura)
		self.menorTemp = str(menor_Temperatura)
		self.maiorUmid = str(maior_Umidade)
		self.menorUmid = str(menor_Umidade)
	def __init__(self, **kwargs):
		super(ReportScreen,self).__init__(**kwargs)
		Clock.schedule_once(self.settings)
		Clock.schedule_interval(self.settings,8)
class TemperatureScreen(Screen):
	count = 0
	oldData1 = StringProperty()
	data = StringProperty()
	sound = SoundLoader.load("notificacao.wav")
	oldData1 = "0"
 	def __init__(self, **kwargs):
		super(TemperatureScreen, self).__init__(**kwargs)
		Clock.schedule_once(self.sensoresTemperatura)
		Clock.schedule_interval(self.sensoresTemperatura,15)
	def sensoresTemperatura(self,i):
		url = "http://192.168.1.177:12345"
		response = urllib.urlopen(url)
		data1 = json.loads(response.read())
		global maior_Temperatura
		global menor_Temperatura
		if float(data1[1]["Temperatura"]) > maior_Temperatura:
			maior_Temperatura = float(data1[1]["Temperatura"])
		if float(data1[1]["Temperatura"]) < menor_Temperatura:
			menor_Temperatura = float(data1[1]["Temperatura"])
		diferenca = float(data1[1]["Temperatura"]) - float(self.oldData1)
		if (diferenca >= 5 or diferenca <= -5) and self.count > 0:
			notification.notify("Alerta de Mudanca","A temperatura da sua casa mudou muito em pouco tempo")
			self.sound.play()
		self.oldData1 = str(data1[1]["Temperatura"])
		if self.oldData1 != self.data:
			self.data  = str(data1[1]["Temperatura"])
		if self.count == 0:
			self.count += 1
class MotionScreen(Screen):
	movimento = StringProperty()
	sound = SoundLoader.load("notificacao.wav")
	def __init__(self, **kwargs):
		super(MotionScreen,self).__init__(**kwargs)
		Clock.schedule_once(self.sensorMovimento)
		Clock.schedule_interval(self.sensorMovimento,5)
	def sensorMovimento(self,i):
		url = "http://192.168.1.177:12345"
		response = urllib.urlopen(url)
		data1 = json.loads(response.read())
		if int(data1[2]["Movimento"]) == 1:
			notification.notify("Alerta de Movimento","Movimento detectado na sua Residencia")
			self.sound.play()
			self.movimento = "Um movimento foi detectado na sua residencia as" + datetime.datetime.now().strftime("%H:%M:%S")
class UmitureScreen(Screen):
	count = 0
	umidade = StringProperty()
	oldData = StringProperty()
	oldData = "0"
	sound = SoundLoader.load("notificacao.wav")
	def __init__(self, **kwargs):
		super(UmitureScreen,self).__init__(**kwargs)
		Clock.schedule_once(self.sensoresUmidade)
		Clock.schedule_interval(self.sensoresUmidade,25)
	def sensoresUmidade(self, i):
		url = "http://192.168.1.177:12345"
		response = urllib.urlopen(url)
		data1 = json.loads(response.read())
		diferenca = float(data1[0]["Umidade"]) - float(self.oldData)
		global maior_Umidade
		global menor_Umidade
		if float(data1[0]["Umidade"]) > maior_Umidade:
			maior_Umidade = float(data1[0]["Umidade"])
		if float(data1[0]["Umidade"]) < menor_Umidade:
			menor_Umidade = float(data1[0]["Umidade"])
		if (diferenca >= 5 or diferenca <= -5) and self.count > 0:
			notification.notify("Alerta de Mudanca","A umidade da sua casa mudou muito em pouco tempo")
			self.sound.play()
		self.oldData = str(data1[0]["Umidade"])
		if self.oldData != self.umidade:
			self.umidade = str(data1[0]["Umidade"])
		if self.count == 0:
			self.count += 1
presentation = Builder.load_file("layout.kv")
class MyApp(App):
	def build(self):
		return presentation

if __name__=="__main__":
	MyApp().run()


