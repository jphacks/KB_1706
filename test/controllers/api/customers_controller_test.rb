require 'test_helper'

class Api::CustomersControllerTest < ActionDispatch::IntegrationTest
  setup do
    @api_customer = api_customers(:one)
  end

  test "should get index" do
    get api_customers_url, as: :json
    assert_response :success
  end

  test "should create api_customer" do
    assert_difference('Api::Customer.count') do
      post api_customers_url, params: { api_customer: { coinadress: @api_customer.coinadress, onepass: @api_customer.onepass, twopass: @api_customer.twopass } }, as: :json
    end

    assert_response 201
  end

  test "should show api_customer" do
    get api_customer_url(@api_customer), as: :json
    assert_response :success
  end

  test "should update api_customer" do
    patch api_customer_url(@api_customer), params: { api_customer: { coinadress: @api_customer.coinadress, onepass: @api_customer.onepass, twopass: @api_customer.twopass } }, as: :json
    assert_response 200
  end

  test "should destroy api_customer" do
    assert_difference('Api::Customer.count', -1) do
      delete api_customer_url(@api_customer), as: :json
    end

    assert_response 204
  end
end
