import requests
from bs4 import BeautifulSoup
import json

BASE_URL = "https://lab401.com/collections/all-products"
PRODUCTS = []

for page in range(1, 7):  # Adjust if more pages exist
    url = f"{BASE_URL}?page={page}&grid_list=grid-view"
    resp = requests.get(url)
    soup = BeautifulSoup(resp.text, 'html.parser')
    for product in soup.select('div.grid-product__content'):
        title_tag = product.select_one('a.grid-product__title')
        price_tag = product.select_one('span.grid-product__price')
        image_tag = product.find_previous('img')
        desc_tag = product.select_one('div.grid-product__description')
        if title_tag:
            PRODUCTS.append({
                "title": title_tag.text.strip(),
                "url": f"https://lab401.com{title_tag['href']}",
                "price": price_tag.text.strip() if price_tag else None,
                "image": image_tag['src'] if image_tag else None,
                "description": desc_tag.text.strip() if desc_tag else None
            })

with open('CYBERFLIPPER/assets/lab401_catalog.json', 'w', encoding='utf-8') as f:
    json.dump(PRODUCTS, f, indent=2, ensure_ascii=False)
