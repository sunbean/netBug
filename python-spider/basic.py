import urllib.request as urllib2
from bs4 import  BeautifulSoup
import re
from urllib.parse import urljoin
#from urllib.parse import urlparse


class HtmlDownloader(object):
    def download(self, url):
        if (url is None) :
            return
        reponse = urllib2.urlopen(url)
        if (reponse.getcode() != 200):
            return None
        return reponse.read()
        
        
class HtmlOutputer(object):
    def __init__(self):
        self.datas = []
    
    def collect_data(self, data):
        if data is None:
            return
        self.datas.append(data)
    def output_html(self):
        fout = open('output.html', 'w', encoding='utf-8')
        fout.write("<html><meta charset=\"utf-8\" />")
       # fout = open('output.html', 'w')
        #fout.write("<html>")
        fout.write("<body>")
        fout.write("<table>")
        for data in self.datas:
            fout.write("<tr>")
            fout.write("<td>%s</td>"% data['url'])
            fout.write("<td>%s</td>"% data['title'])
            #fout.write("<td>%s</td>"% data['summary'])
            fout.write("</tr>")
        fout.write("</table>")
        fout.write("</body>")
        fout.write("</html>")
        fout.close()



class HtmlParser(object):
    def _get_new_urls(self, page_url, soup):
        new_urls = set()
        #/html/article/index5181.html
        links = soup.find_all('a', href=re.compile(r"/html/article/index\d+.html"))
        for link in links:
            new_url = link['href']
            new_full_url = urljoin(page_url, new_url)
            new_urls.add(new_full_url)
            
        return new_urls

    def _get_new_data(self, page_url, soup):
        res_data = {}
        res_data['url'] = page_url
        #<dd class="lemmaWgt-lemmaTitle-title"><h1>Python</h1>
        #<a class="mn" href="/"><font color="#FF0000">※小说区※</font></a>
        title_node = soup.find('a', class_="mn").find("font")
        res_data['title'] = title_node.get_text()
        #<div class="lemma-summary" label-module="lemmaSummary">
        #<div class="mainArea px9">
        #summary_node = soup.find('div', class_="mainArea px9").find("br")
        #res_data['summary'] = summary_node.get_text()
        return res_data
        
    def parse(self, page_url, html_cont):
        if (page_url is None or html_cont is None):
            return
        soup = BeautifulSoup(html_cont, "html.parser", from_encoding='utf-8')
        new_urls = self._get_new_urls(page_url, soup)
        new_data = self._get_new_data(page_url, soup)
        return new_urls, new_data
        
class UrlManager(object) :
    def __init__(self):
        self.new_urls = set()
        self.old_urls = set()
    def add_new_url(self, url) :
        if (url is None):
            return
        if (url not in self.new_urls and url not in self.old_urls):
            self.new_urls.add(url)
                
    def add_new_urls(self, urls):
        if (urls is None or len(urls) == 0):
            return
        for url in urls :
            self.add_new_url(url)
            
    def has_new_url(self):
        return len(self.new_urls) != 0;
    def get_new_url(self):
        new_url = self.new_urls.pop()
        self.old_urls.add(new_url)
        return new_url
        
class SpiderMain():
    def __init__(self):
        self.urls = UrlManager()
        self.downloader = HtmlDownloader()
        self.parser = HtmlParser()
        self.output = HtmlOutputer()
        
        
    def craw(self, root_url):
        self.urls.add_new_url(root_url)
        count = 1
        while self.urls.has_new_url():
            try:  
                new_url  = self.urls.get_new_url()
                print ('craw %d : %s' % (count, new_url))
                html_cont = self.downloader.download(new_url)
                new_urls, new_data = self.parser.parse(new_url, html_cont)
                self.urls.add_new_urls(new_urls)
                self.output.collect_data(new_data)
                if (count == 20) :
                    break
                count = count + 1
            except:
                print ('craw failed')
        self.output.output_html()
if (__name__=="__main__"):
    root_url = "http://www.77c2.com/"
    obj_spider = SpiderMain()
    obj_spider.craw(root_url)
