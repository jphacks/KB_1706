Rails.application.routes.draw do
  namespace :api do
    resources :customers
  end
    namespace :api do
    mount_devise_token_auth_for 'User', at: 'auth'
  end
end
