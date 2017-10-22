Rails.application.routes.draw do
  namespace :api do
    resources :customers do
    	post :pay, on: :collection
    end
  end
    namespace :api do
    mount_devise_token_auth_for 'User', at: 'auth'
  end
end
