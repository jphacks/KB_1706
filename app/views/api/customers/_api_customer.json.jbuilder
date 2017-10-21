json.extract! api_customer, :id, :coinadress, :onepass, :twopass, :created_at, :updated_at
json.url api_customer_url(api_customer, format: :json)
